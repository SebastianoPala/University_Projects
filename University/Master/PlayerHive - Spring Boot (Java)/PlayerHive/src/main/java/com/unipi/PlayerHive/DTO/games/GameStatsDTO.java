package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Field;

import java.util.List;

@Getter
@AllArgsConstructor
@NoArgsConstructor
public class GameStatsDTO {
    @Id
    private String id;

    private String name;

    private Double price;
    private Integer discount;

    private Double finalPrice;

    private Double avgRating;


    @Field("image")
    private String imageURL;

    public List<String> genres;
}
